import pykitti
import vtk_anim
import numpy as np
from tqdm import tqdm
import vtk_pc_plot

basedir = '/home/ran/Kitti'
date = '2011_09_26'
drive = '0009'
ld_list_f = 'lidar_list.npy'
data = pykitti.raw(basedir, date, drive)

poses = []
velo_imu = data.calib.T_velo_imu
imu_velo = np.linalg.inv(velo_imu)
velo0 = data.get_velo(0)
n_dataset = len(data.oxts)
lidar_list = []
# for ii in tqdm(range(len(data.velo_files))):
#     veloi = data.get_velo(ii)
#     w_velo0 = []
#     for ind, vel in enumerate(veloi):
#         # imu_point = np.dot(imu_velo, vel)
#         # Tw_imu = data.oxts[ii].T_w_imu
#         # w_point = np.dot(Tw_imu, imu_point)
#         w_point = np.dot(data.oxts[ii].T_w_imu, vel)
#         w_velo0.append(w_point)
#     lidar_list.append(w_velo0)
# lidar_list = np.array(lidar_list)
# np.save(ld_list_f, lidar_list)
dsize = len(data.velo_files)
# lidar_list = np.load(ld_list_f)
# vtk_anim.render_kitti_velo(iter(lidar_list), dsize)


tw_imu = data.oxts[0].T_w_imu
ldptrs = []
for point in velo0:
    p_imu = np.dot(imu_velo, point)
    ldptrs.append(np.dot(tw_imu, p_imu))

vtk_pc_plot.plot_point_cloud(ldptrs)