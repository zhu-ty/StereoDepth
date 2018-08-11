clc;
clear;

dir = 'E:/data/stereo/No2';
depth_name = 'depth_center1_warp.tiff';
out_name = 'depth_center1_warp_refined.png';

depth = double(imread([dir,'/',depth_name]));
smooth = depth_refine(depth);
imwrite(uint16(smooth),[dir,'/',out_name]);