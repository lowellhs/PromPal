@everywhere using DistributedArrays, LinearAlgebra
using CUDAnative, CuArrays, CUDAdrv
using BenchmarkTools, Test

@everywhere function matmul(A, B)
    return A*B
end

@everywhere function matmul_par(A, B)
    A_rows, _ = size(A)
    _, B_cols = size(B)
    C = zeros(A_rows, B_cols)
    @sync begin
        for p in workers()
            @async begin
                rows, _ = @fetchfrom p localindices(A)
                A_parts = @fetchfrom p localpart(A)
                future_res = remotecall(matmul, p, A_parts, B)
                C[rows, :] = fetch(future_res)
            end
        end
    end
    return C
end

n     = parse(Int, ARGS[1])
A_seq = Matrix{Int}(I,n,n)
B_seq = randn(n,n)

# Multicore data
A_par = distribute(A_seq, procs=workers(), dist=[nworkers(), 1])

# GPU data
A_gpu = CuMatrix(A_seq)
B_gpu = CuMatrix(B_seq)

C_seq = @btime matmul(A_seq, B_seq) samples=1 evals=1
C_par = @btime matmul_par(A_par, B_seq) samples=1 evals=1
C_gpu = @btime A_gpu*B_gpu samples=1 evals=1

println("Test equality: ", @test all(C_seq .== C_par .== Array(C_gpu)))
