import pykitti
import vtk_anim
import numpy as np
from tqdm import tqdm
import vtk_pc_plot
import open3d as o3d


basedir = '/home/ran/Kitti'
date = '2011_09_26'
drive = '0009'
ld_list_f = 'lidar_list.npy'
data = pykitti.raw(basedir, date, drive)
velo0 = data.get_velo(0)

pcd = o3d.geometry.PointCloud()
pcd.points = o3d.utility.Vector3dVector(velo0[:, 0:3])
o3d.visualization.draw_geometries([pcd])