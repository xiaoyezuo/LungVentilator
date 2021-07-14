import open3d as o3d
import numpy as np

class AzureKinect:
    def __init__(self):
        self.config = o3d.io.AzureKinectSensorConfig()
        self.device = 0
        self.align_depth_to_color = 1

    def start(self):
        self.sensor = o3d.io.AzureKinectSensor(self.config)
        if not self.sensor.connect(self.device):
            raise RuntimeError('Failed to connect to sensor')

    def frames(self):
            while 1:
                rgbd = self.sensor.capture_frame(self.align_depth_to_color)
                if rgbd is None:
                    continue
                color,depth = np.asarray(rgbd.color).astype(np.uint8),np.asarray(rgbd.depth).astype(np.float32) / 1000.0
                return color, depth

cam = AzureKinect()
cam.start()
color, depth = cam.frames()
intrinsic = o3d.camera.PinholeCameraIntrinsic(1280, 720, 601.1693115234375, 600.85931396484375, 637.83624267578125, 363.8018798828125)
depth = o3d.geometry.Image(depth)
img = o3d.geometry.Image(color)
rgbd = o3d.geometry.RGBDImage.create_from_color_and_depth(img, depth, depth_scale=1.0, convert_rgb_to_intensity=False)
pcd = o3d.geometry.PointCloud.create_from_rgbd_image(rgbd, intrinsic)
o3d.visualization.draw_geometries([pcd])