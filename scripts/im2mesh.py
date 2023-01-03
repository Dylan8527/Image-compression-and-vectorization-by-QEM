'''
Author: Dylan8527 vvm8933@gmail.com
Date: 2022-11-10 00:31:14
LastEditors: Dylan8527 vvm8933@gmail.com
LastEditTime: 2022-11-14 17:24:31
FilePath: \CS271assignment3\scripts\im2mesh.py
Description: load RGB image and save it into triangeluar mesh

Copyright (c) 2022 by Dylan8527 vvm8933@gmail.com, All Rights Reserved. 
'''
import trimesh
import numpy as np
import matplotlib.pyplot as plt
from skimage.transform import resize

def example():
    vertices = np.random.rand(10, 3)
    print(vertices)
    faces = np.random.randint(0, 10, (20, 3))
    vertex_colors = np.random.rand(10, 3)
    mesh = trimesh.Trimesh(vertices=vertices, faces=faces, vertex_colors=vertex_colors)
    print(mesh)
    mesh.show()

if __name__ == "__main__":
    # read jpg image
    img = plt.imread('../data/ray_tracing_64.png')  
    # first downsample it to 64x64
    img = resize(img, (64, 64), anti_aliasing=True)
    # show image
    plt.imshow(img)
    plt.show()


    # rotate the image by 90 degree
    img = np.rot90(img, 3, (0, 1))
    print(img.shape)
    # convert to mesh
    vertex_color = img.reshape(-1, 3)
    # first generate vertex position
    y, x = np.meshgrid(np.arange(img.shape[1]), np.arange(img.shape[0]))
    x = x.flatten() / (img.shape[0] - 1) * 2 - 1
    y = y.flatten() / (img.shape[1] - 1) * 2 - 1
    z = np.ones_like(x) * -5
    vertices = np.stack([x, y, z], axis=1)
    normals = np.zeros_like(vertices)
    normals[:, 2] = 1
    print(f"vertices: {vertices.shape}")
    print(f"normals: {normals.shape}")
    print(f"vertex_color: {vertex_color.shape}")
    # print(vertices)

    # then generate faces
    faces = []
    for i in range(img.shape[0]-1):
        for j in range(img.shape[1]-1):
            faces.append([[i*img.shape[1]+j, i*img.shape[1]+j+1, (i+1)*img.shape[1]+j],
                              [i*img.shape[1]+j+1, (i+1)*img.shape[1]+j+1, (i+1)*img.shape[1]+j]])
    faces = np.array(faces).reshape(-1, 3)

    print(f"faces: {faces.shape}")
    
    # create mesh
    mesh = trimesh.Trimesh(vertices=vertices, faces=faces, vertex_colors=vertex_color, vertex_normals=normals)
    # mesh.show()

    # save mesh
    mesh.export('../data/panic.obj')




