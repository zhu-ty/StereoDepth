clc;
clear;

dir = 'E:/data/inpaint_test';
disparity_name = 'disparity.tiff';
pt = [500;600];%x,y
d = 2000;%cm

% dir = 'E:/data/stereo/No1';
% disparity_name = 'disparity.tiff';
% pt = [430;1050];%x,y
% d = 4000;%cm
% K = [ 8.00883728e+02, 0., 1000., 0., 8.00883728e+02, 750., 0., 0., ...
%        1. ];

I_dis = imread([dir,'/',disparity_name]);
I_dis = double(I_dis);


I_depth = 1./I_dis;
I_depth_center = I_depth;


% Use real world to modify the depth
% Depth unit: cm

I_depth_center_fixed = I_depth_center * (d / I_depth_center(pt(2),pt(1)));
I_depth_center_fixed(I_depth_center_fixed > 50000) = 50000;
imshow(I_depth_center_fixed,[]);

writeftif(I_depth_center_fixed, [dir,'/','depth_DepthOnly.tiff']);