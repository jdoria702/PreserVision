import numpy as np
from sklearn.cluster import KMeans

path = 'images/output/laws_texture/LawsTexture_energy_values.csv'

# load CSV (skip header)
data = np.loadtxt(path, delimiter=',', skiprows=1)
if data.ndim == 1:
    data = data.reshape(1, -1)   # handle single row

# first column = patch_id, rest = features
patch_ids = data[:, 0].astype(int)
X = data[:, 1:].astype(float)

# run KMeans
kmeans = KMeans(n_clusters=5, random_state=42)
labels = kmeans.fit_predict(X)

# save patch_id + label with header
out = np.column_stack((patch_ids, labels))
np.savetxt('images/output/laws_texture/kmeans_labels.csv', out, fmt='%d', delimiter=',',
           header='patch_id,label', comments='')