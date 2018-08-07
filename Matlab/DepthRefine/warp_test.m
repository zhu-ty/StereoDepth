clc;
clear;
load('matlab.mat');
%resize
vx_r = imresize(vx, [3008,4112]);
vy_r = imresize(vy, [3008,4112]);

[M,N]=size(vx_r);
[x,y]=meshgrid(1:N,1:M);
x = x + vx_r;
y = y + vy_r;


I1 = imread('opt_00.jpg');
I1 = imresize(I1, [3008,4112]);
I2 = imread('opt_01.jpg');
I2 = imresize(I2, [3008,4112]);

I3 = warpFLColor(I1,I2,vx_r,vy_r);

% figure(1);
% imshow(I1,[]);
% figure(2);
% imshow(I2,[]);
% figure(3);
% imshow(I3,[]);

clear volume;
volume(:,:,:,1) = I1;
volume(:,:,:,2) = I2;
volume(:,:,:,3) = im2uint8(I3);
if exist('output','dir')~=7
    mkdir('output');
end
frame2gif(volume,fullfile('output',['warp.gif']));
