addpath('OpticalFlow_LiuCe');
addpath('OpticalFlow_LiuCe/mex');

for i = 1:10
    
    fprintf('working on %d ...\n', i);
    
    dir = ['../',num2str(i)];

    pic_master = '!L.jpg';
    pic_slave = '!R.jpg';

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
end