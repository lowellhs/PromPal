from keras.backend.tensorflow_backend import set_session
import os
import tensorflow as tf


def set_cpu_option():
    os.environ["CUDA_DEVICE_ORDER"] = "PCI_BUS_ID"
    os.environ["CUDA_VISIBLE_DEVICES"] = ""


def set_gpu_option(which_gpu):
     gpu_devices = tf.config.experimental.list_physical_devices('GPU')
     tf.config.experimental.set_visible_devices(gpu_devices[which_gpu], 'GPU')
     tf.config.experimental.set_memory_growth(gpu_devices[which_gpu], True)