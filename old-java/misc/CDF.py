import pandas as pd
import matplotlib as mpl 
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns
from matplotlib.patches import Polygon

# Read
bigProb = pd.read_csv('200k-experiment-big.txt') 
smallProb = pd.read_csv('200k-experiment-little.txt')

# Preprocess
took_ms = np.array(bigProb['took_ms'].values.tolist())
bigProb['took_ms'] = np.where(took_ms > 3000, 3000, took_ms).tolist()
took_ms = np.array(smallProb['took_ms'].values.tolist())
smallProb['took_ms'] = np.where(took_ms > 3000, 3000, took_ms).tolist()

# CDF Code

N = 199999
A = bigProb['took_ms']
B = smallProb['took_ms']


H,X1 = np.histogram(A, bins = 10, normed = True)
X2 = np.sort(A)
F2 = np.array(range(N))/float(N)
markers_on = np.percentile(bigProb['took_ms'], 95)
plt.plot(X2, F2, label='Big')
plt.rc('xtick', labelsize=12) 
plt.rc('ytick', labelsize=12) 
H,X1 = np.histogram(B, bins = 10, normed = True)
X2 = np.sort(B)
F2 = np.array(range(N))/float(N)
markers_on = np.percentile(smallProb['took_ms'], 95)

fig = plt.figure(1, figsize=(10, 6))
plt.plot(X2, F2, '--', label='Small')

legend = plt.legend(loc='lower right')
plt.xlabel('Service Time', size='large')
plt.ylabel('Probability', size='large')
plt.title('Cumulative Distributive Function')
plt.annotate('95%', xy=(np.percentile(bigProb['took_ms'], 95), 0.94), xytext=(110,0.8),
            arrowprops=dict(facecolor='black', width=1, headwidth=7, shrink=0.01),
            )
plt.annotate('95%', xy=(np.percentile(smallProb['took_ms'], 95), 0.94), xytext=(600, 0.8),
            arrowprops=dict(facecolor='black', width=1, headwidth=7, shrink=0.01),
            )

plt.show()

fig.savefig('CDF200k_BigvsSmall.png', bbox_inches='tight')
