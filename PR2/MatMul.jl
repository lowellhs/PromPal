@everywhere using DistributedArrays
@everywhere using LinearAlgebra
@everywhere using BenchmarkTools

@everywhere function matmul(A, B)
    A_rows, A_cols = size(A)
    B_rows, B_cols = size(B)
    C = zeros(A_rows, B_cols)
    for i = 1:A_rows
        for j = 1:B_cols
            for k = 1:B_rows
                C[i, j] += A[i, k] * B[k, j]
            end
        end
    end
    return C
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

function checkIsEqual(A, B)
    if length(A) != length(B)
        return "Not Equal"
    end
    for i=1:length(A)
        if abs(A[i] - B[i]) > 1e-6
            return "Not Equal"
        end
    end
    return "Equal"
end

n   = parse(Int, ARGS[1])
A   = Matrix{Int}(I,n,n)
A_d = distribute(A, procs=workers(), dist=[nworkers(), 1])
B   = randn(n,n)

C_seq = @btime matmul(A, B)
C_par = @btime matmul_par(A_d, B)
println("Sequential and Parallel are ", checkIsEqual(C_seq, C_par))
