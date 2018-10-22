clc;
clear;

dir = 'E:/data/inpaint_test';
depth_name = 'depth_DepthOnly.tiff';
out_name = 'depth_DepthOnly.png';

depth = double(imread([dir,'/',depth_name]));
%smooth = depth_refine(depth);
imwrite(uint16(depth),[dir,'/',out_name]);