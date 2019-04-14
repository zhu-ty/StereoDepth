function [vxabs,vyabs] = flowfunc(LimgName,RimgName, alpha, ratio)

addpath('mex');

dir = '.';

pic_master = LimgName;
pic_slave = RimgName;

% load the two frames
im1 = im2double(imread([dir,'/',pic_master]));
im2 = im2double(imread([dir,'/',pic_slave]));

im2 = imresize(im2, [size(im1,1),size(im1,2)]);


% set optical flow parameters (see Coarse2FineTwoFrames.m for the definition of the parameters)
% building: alpha=0.08, ratio = 0.9
% default: alpha=0.012, ratio = 0.75
% alpha = 0.012;
% ratio = 0.75;
minWidth = 20;
nOuterFPIterations = 7;
nInnerFPIterations = 1;
nSORIterations = 30;

para = [alpha,ratio,minWidth,nOuterFPIterations,nInnerFPIterations,nSORIterations];

% this is the core part of calling the mexed dll file for computing optical flow
% it also returns the time that is needed for two-frame estimation
tic;
[vx,vy, warpI2] = Coarse2FineTwoFrames(im1,im2,para);
toc

save([dir,'/','flow_vx_vy.mat'], 'pic_master', 'pic_slave', 'vx','vy');
vxabs = abs(vx);
vyabs = abs(vy);
writeftif(vxabs,[dir,'/','disparity_vx.tiff']);
writeftif(vyabs,[dir,'/','disparity_vy.tiff']);
end

