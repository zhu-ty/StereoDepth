clc;
clear;

dir = 'E:/data/20190110';
disparity_name = 'disparity.tiff';
pt = [500;600];%x,y
d = 2000;%cm
K = [ 8.24600342e+02, 0., 505., 0., 8.24600342e+02, 605., 0., 0.,  ...
       1. ];

% dir = 'E:/data/stereo/No1';
% disparity_name = 'disparity.tiff';
% pt = [430;1050];%x,y
% d = 4000;%cm
% K = [ 8.00883728e+02, 0., 1000., 0., 8.00883728e+02, 750., 0., 0., ...
%        1. ];

K = reshape(K,[3,3])';
I_dis = imread([dir,'/',disparity_name]);
I_dis = double(I_dis);
[height,width] = size(I_dis);
K = K * ((width/2)/K(1,3));
K = [K(:,:),[0;0;0]];
K = [K(:,:);[0,0,0,1]];
K(3,3) = 1.0;
Ki = inv(K);

I_depth = 1./I_dis;
I_depth_center = I_depth;

for i = 1:height
    for j = 1:width
        depthVal = I_depth(i,j);
        vec = Ki * [depthVal * j; depthVal *i; depthVal;1.0];
        I_depth_center(i,j) = norm(vec (1:3),2);
    end
end

% Use real world to modify the depth
% Depth unit: cm

I_depth_center_fixed = I_depth_center * (d / I_depth_center(pt(2),pt(1)));
I_depth_center_fixed(I_depth_center_fixed > 50000) = 50000;
imshow(I_depth_center_fixed,[]);

writeftif(I_depth_center_fixed, [dir,'/','depth_center.tiff']);