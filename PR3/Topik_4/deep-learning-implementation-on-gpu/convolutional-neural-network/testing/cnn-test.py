from __future__ import print_function


import keras
from keras.datasets import mnist
from keras.models import Sequential
from keras.layers import Dense, Dropout, Flatten
from keras.layers import Conv2D, MaxPooling2D
from keras import backend as K
import os
from set_options import set_cpu_option, set_gpu_option
import time

print('''
CPU or GPU?
Type cpu and hit enter to use cpu.
Type gpu # (0 for NVIDIA GeForce GTX 1080) and hit enter to use gpu.
Command example for GPU: gpu 0
''')

option = input('Enter command: ', )

if(option == 'cpu'):
    print('using cpu')
    set_cpu_option()
elif(option.split()[0] == 'gpu'):
    print('using gpu ', option.split()[1])
    set_gpu_option(int(option.split()[1]))

start_time = time.time()
batch_size = 128
num_classes = 10
epochs = 12

# input image dimensions
img_rows, img_cols = 28, 28

# the data, split between train and test sets
(x_train, y_train), (x_test, y_test) = mnist.load_data()

if K.image_data_format() == 'channels_first':
    x_train = x_train.reshape(x_train.shape[0], 1, img_rows, img_cols)
    x_test = x_test.reshape(x_test.shape[0], 1, img_rows, img_cols)
    input_shape = (1, img_rows, img_cols)
else:
    x_train = x_train.reshape(x_train.shape[0], img_rows, img_cols, 1)
    x_test = x_test.reshape(x_test.shape[0], img_rows, img_cols, 1)
    input_shape = (img_rows, img_cols, 1)

x_train = x_train.astype('float32')
x_test = x_test.astype('float32')
x_train /= 255
x_test /= 255
print('x_train shape:', x_train.shape)
print(x_train.shape[0], 'train samples')
print(x_test.shape[0], 'test samples')

# convert class vectors to binary class matrices
y_train = keras.utils.to_categorical(y_train, num_classes)
y_test = keras.utils.to_categorical(y_test, num_classes)

model = Sequential()
model.add(Conv2D(32, kernel_size=(3, 3),
                 activation='relu',
                 input_shape=input_shape))
model.add(Conv2D(64, (3, 3), activation='relu'))
model.add(MaxPooling2D(pool_size=(2, 2)))
model.add(Dropout(0.25))
model.add(Flatten())
model.add(Dense(128, activation='relu'))
model.add(Dropout(0.5))
model.add(Dense(num_classes, activation='softmax'))

model.compile(loss=keras.losses.categorical_crossentropy,
              optimizer=keras.optimizers.Adadelta(),
              metrics=['accuracy'])

model.fit(x_train, y_train,
          batch_size=batch_size,
          epochs=epochs,
          verbose=1,
          validation_data=(x_test, y_test))
score = model.evaluate(x_test, y_test, verbose=0)
print('Test loss: {score:.4f}'.format(score = score[0]))
print('Test accuracy: {score:.4f}'.format(score = score[1]))
# All epochs execution time included.
print('Execution Time: {execution_time:.4f} seconds.'.format(execution_time = (time.time() - start_time)))

def write_file(file, device_name, file_count):
    file.write("This is the result number {0} for {1}.\n".format(file_count, device_name))
    file.write("Number of epochs: ", epochs)
    file.write('x_train shape:', x_train.shape)
    file.write(x_train.shape[0], 'train samples')
    file.write(x_test.shape[0], 'test samples\n')
    file.write('Test loss: {score:.4f}'.format(score = score[0]))
    file.write('Test accuracy: {score:.4f}'.format(score = score[1]))
    file.write('Execution Time: {execution_time:.4f} seconds.'.format(execution_time = (time.time() - start_time)))
    file.close()

THIS_FOLDER = os.path.dirname(os.path.abspath(__file__))
is_null = False

if(input == 'cpu'):
    device_name = 'CPU'
    RESULT_PATH = os.path.join(THIS_FOLDER, 'results/CPU_results/')
    if not os.path.exists(RESULT_PATH):
        os.makedirs(RESULT_PATH)
        file_count = 1
        is_null = True
    
    if is_null == False:
        file_count = len(RESULT_PATH)
    result_file = os.path.join(RESULT_PATH, 'results_CPU_#{0}.txt'.format(file_count))
    file = open(result_file, 'w') 
    write_file(file, device_name, file_count)

elif(option.split()[0] == 'gpu'): # Currently only GTX 1080 available
    device_name = 'NVIDIA GeForce GTX 1080 GPU'
    RESULT_PATH = os.path.join(THIS_FOLDER, 'results/GTX1080_results/')
    if not os.path.exists(RESULT_PATH):
        os.makedirs(RESULT_PATH)
        file_count = 1
        is_null = True

    if is_null == False:
        file_count = len(RESULT_PATH)
    result_file = os.path.join(RESULT_PATH, 'results_GTX1080_#{0}.txt'.format(file_count))
    file = open(result_file, 'w') 
    write_file(file, device_name, file_count)