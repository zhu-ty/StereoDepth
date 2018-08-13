clc;
clear;

dir = 'E:/data/stereo/No1';
depth_name = 'depth_center0_warp.tiff';
out_name = 'depth_center0_warp_refined.png';

depth = double(imread([dir,'/',depth_name]));
smooth = depth_refine(depth);
imwrite(uint16(smooth),[dir,'/',out_name]);