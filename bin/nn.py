import numpy as np
import pandas as pd
import tensorflow as tf
from sklearn import preprocessing
from tensorflow import keras
from tensorflow.keras import layers

def buildDeadlineModel():
  model = keras.Sequential([
          layers.Dense(64, activation='relu', input_shape=[len(train_dataset.keys())]),
          layers.Dense(16, activation='relu'),
          layers.Dense(4, activation='relu'),
          layers.Dense(1, activation='sigmoid')
          ])
  optimizer = tf.keras.optimizers.RMSprop(0.001)
  model.compile(loss='binary_crossentropy', optimizer=optimizer, metrics=['accuracy'])
  return model

def buildEnergyModel():
  model = keras.Sequential([
          layers.Dense(64, activation='relu', input_shape=[len(train_dataset.keys())]),
          layers.Dense(16, activation='relu'),
          layers.Dense(4, activation='relu'),
          layers.Dense(1)
          ])
  optimizer = tf.keras.optimizers.RMSprop(0.001)
  model.compile(loss='mse', optimizer=optimizer, metrics=['mae', 'mse'])
  return model

def norm(x, train_stats):
      return (x - train_stats['mean']) / train_stats['std']

def deadlineData(db):
    db['deadline'] = np.where(db['p95'] > 7, 0, 1)
    db.drop(db[db[' energyconsumption'] < 0].index, inplace=True)
    db[' energyconsumption'] = db[' energyconsumption']/1000000
    energy = db.pop(' energyconsumption')
    db[' qps'] = db[' qps']/100

    # One hot on cpufreq
    cpufreq = db.pop(' cpufreq')
    le = preprocessing.LabelEncoder()
    le.fit(cpufreq)
    catCpuFreq = le.transform(cpufreq)

    # Remove p95, p99
    db.pop('p95')
    db.pop(' p99')

    db['1.0G'] = (catCpuFreq == 0)*1.0
    db['1.2G'] = (catCpuFreq == 1)*1.0
    db['1.3G'] = (catCpuFreq == 2)*1.0
    db['1.5G'] = (catCpuFreq == 3)*1.0
    db['1.7G'] = (catCpuFreq == 4)*1.0
    db['1.9G'] = (catCpuFreq == 5)*1.0
    db['2.1G'] = (catCpuFreq == 6)*1.0
    db['2.3G'] = (catCpuFreq == 7)*1.0
    db['2.5G'] = (catCpuFreq == 8)*1.0

    train_dataset = db.sample(frac=0.7,random_state=0)
    train_stats = train_dataset.describe()
    train_stats.pop("deadline")
    train_stats = train_stats.transpose()
    train_labels = train_dataset.pop('deadline')

    normed_train_data = norm(train_dataset)
    return normed_train_data, train_labels

def energyData(db):
    db['deadline'] = np.where(db['p95'] > 7, 0, 1)

    # Remove negative energy and transform to kJ
    db.drop(db[db[' energyconsumption'] < 0].index, inplace=True)
    db[' energyconsumption'] = db[' energyconsumption']/1000000
    #energy = db.pop(' energyconsumption')

    # Reduce qps by 100
    db[' qps'] = db[' qps']/100

    # One hot on cpufreq
    cpufreq = db.pop(' cpufreq')
    le = preprocessing.LabelEncoder()
    le.fit(cpufreq)
    catCpuFreq = le.transform(cpufreq)

    # Remove p95, p99
    db.pop('p95')
    db.pop(' p99')

