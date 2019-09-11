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

numDists = 11
N = 199999
dists = ['1', '2', '3', '4', '5', '6', '7', '11', '12', '13', '14']

kw1 = bigProb[bigProb.kw_lengh == 1]
kw1 = kw1['took_ms'].values
kw2 = bigProb[bigProb.kw_lengh == 2]
kw2 = kw2['took_ms'].values
kw3 = bigProb[bigProb.kw_lengh == 3]
kw3 = kw3['took_ms'].values
kw4 = bigProb[bigProb.kw_lengh == 4]
kw4 = kw4['took_ms'].values
kw5 = bigProb[bigProb.kw_lengh == 5]
kw5 = kw5['took_ms'].values
kw6 = bigProb[bigProb.kw_lengh == 6]
kw6 = kw6['took_ms'].values
kw7 = bigProb[bigProb.kw_lengh == 7]
kw7 = kw7['took_ms'].values
kw8 = bigProb[bigProb.kw_lengh == 8]
kw8 = kw8['took_ms'].values
kw9 = bigProb[bigProb.kw_lengh == 9]
kw9 = kw9['took_ms'].values
kw10 = bigProb[bigProb.kw_lengh == 10]
kw10 = kw10['took_ms'].values
kw11 = bigProb[bigProb.kw_lengh == 11]
kw11 = kw11['took_ms'].values
kw12 = bigProb[bigProb.kw_lengh == 12]
kw12 = kw12['took_ms'].values
kw13 = bigProb[bigProb.kw_lengh == 13]
kw13 = kw13['took_ms'].values
kw14 = bigProb[bigProb.kw_lengh == 14]
kw14 = kw14['took_ms'].values

# Boxplot Code

data = [kw1, kw2, kw3, kw4, kw5, kw6, kw7, kw11, kw12, kw13, kw14]
fig, ax1 = plt.subplots(figsize=(10, 6))
fig.canvas.set_window_title('Takentime vs Keyword')
plt.subplots_adjust(left=0.075, right=0.95, top=0.9, bottom=0.25)

bp = plt.boxplot(data, notch=0, sym='+', vert=1, whis=1.5)
plt.setp(bp['boxes'], color='black')
plt.setp(bp['whiskers'], color='black')
plt.setp(bp['fliers'], color='red', marker='+')
ax1.yaxis.grid(True, linestyle='-', which='major', color='lightgrey',
               alpha=0.5)

# Hide these grid behind plot objects
ax1.set_axisbelow(True)
ax1.set_title('Time Taken per Keyword')
ax1.set_xlabel('Keyword Lenght')
ax1.set_ylabel('Took Time')

# Now fill the boxes with desired colors
boxColors = ['darkkhaki', 'royalblue']
numBoxes = 11
medians = list(range(numBoxes))
for i in range(numBoxes):
    box = bp['boxes'][i]
    boxX = []
    boxY = []
    for j in range(5):
        boxX.append(box.get_xdata()[j])
        boxY.append(box.get_ydata()[j])
    boxCoords = list(zip(boxX, boxY))
    # Alternate between Dark Khaki and Royal Blue
    k = i % 2
    boxPolygon = Polygon(boxCoords, facecolor=boxColors[k])
    ax1.add_patch(boxPolygon)
    # Now draw the median lines back over what we just filled in
    med = bp['medians'][i]
    medianX = []
    medianY = []
    for j in range(2):
        medianX.append(med.get_xdata()[j])
        medianY.append(med.get_ydata()[j])
        plt.plot(medianX, medianY, 'k')
        medians[i] = medianY[0]
    # Finally, overplot the sample averages, with horizontal alignment
    # in the center of each box
    plt.plot([np.average(med.get_xdata())], [np.average(data[i])],
             color='w', marker='*', markeredgecolor='k')

# Set the axes ranges and axes labels
ax1.set_xlim(0.5, numBoxes + 0.5)
top = 3000
bottom = -200
ax1.set_ylim(bottom, top)
xtickNames = plt.setp(ax1, xticklabels=dists)
plt.setp(xtickNames, rotation=45, fontsize=8)

# Due to the Y-axis scale being different across samples, it can be
# hard to compare differences in medians across the samples. Add upper
# X-axis tick labels with the sample medians to aid in comparison
# (just use two decimal places of precision)
pos = np.arange(numBoxes) + 1
upperLabels = [str(np.round(s, 2)) for s in medians]
weights = ['bold', 'semibold']
for tick, label in zip(range(numBoxes), ax1.get_xticklabels()):
    k = tick % 2
    ax1.text(pos[tick], top - (top*0.05), upperLabels[tick],
             horizontalalignment='center', size='x-small', weight=weights[k],
             color=boxColors[k])

# Finally, add a basic legend
plt.show()
fig.savefig('fig1.png', bbox_inches='tight')
