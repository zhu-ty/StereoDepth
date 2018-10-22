addpath('mex');

dir = 'E:/data/inpaint_test';

pic_master = '0000_left.png';
pic_slave = '0000_right.jpg';

% load the two frames
im1 = im2double(imread([dir,'/',pic_master]));
im2 = im2double(imread([dir,'/',pic_slave]));

im2 = imresize(im2, [size(im1,1),size(im1,2)]);


% set optical flow parameters (see Coarse2FineTwoFrames.m for the definition of the parameters)
alpha = 0.012;
ratio = 0.75;
minWidth = 20;
nOuterFPIterations = 7;
nInnerFPIterations = 1;
nSORIterations = 30;

para = [alpha,ratio,minWidth,nOuterFPIterations,nInnerFPIterations,nSORIterations];

% this is the core part of calling the mexed dll file for computing optical flow
% it also returns the time that is needed for two-frame estimation
tic;
[vx,vy,warpI2] = Coarse2FineTwoFrames(im1,im2,para);
toc

save([dir,'/','flow_vx_vy.mat'], 'vx','vy');
vx_abs = abs(vx);
writeftif(vx_abs,[dir,'/','disparity.tiff']);

clear flow;
flow(:,:,1) = vx;
flow(:,:,2) = vy;
imflow = flowToColor(flow);

figure;imshow(imflow);

% 
% figure;imshow(im1);figure;imshow(warpI2);
% 
% % 
% 
% % output gif
% clear volume;
% volume(:,:,:,1) = im1;
% volume(:,:,:,2) = im2;
% if exist('output','dir')~=7
%     mkdir('output');
% end
% frame2gif(volume,fullfile('output',[example '_input.gif']));
% volume(:,:,:,2) = warpI2;
% frame2gif(volume,fullfile('output',[example '_warp.gif']));
% 
% 
% % visualize flow field
% clear flow;
% flow(:,:,1) = vx;
% flow(:,:,2) = vy;
% imflow = flowToColor(flow);
% 
% figure;imshow(imflow);
% imwrite(imflow,fullfile('output',[example '_flow.jpg']),'quality',100);
