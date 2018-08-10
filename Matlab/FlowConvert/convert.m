clc;
clear;
dir = 'E:/data/stereo/No2';
I1_name = 'CUCAU1731016_Resized_R';
I2_name = 'CUCAU1731034_Resized_R';
load([dir,'/flow_vx_vy.mat']);



rows = size(vx,1);
cols = size(vx,2);

% %rows = 3008;%2056;%3008
% %cols = 4112;%2464;%4112
% 
% rows = 2056;%3008
% cols = 2464;%4112
% 
% vx_r = imresize(vx, [rows,cols]);
% vy_r = imresize(vy, [rows,cols]);

[M,N]=size(vx);
[x,y]=meshgrid(1:N,1:M);
x = x + vx;
y = y + vy;

% matlab2opencv( x, 'warp.yml');
% matlab2opencv( y, 'warp.yml', 'a'); % append mode passed by 'a' flag

%cv::Mat t = cv::imread("E:/data/stereo/No1/x.tiff", cv::IMREAD_ANYDEPTH);
writeftif(x, [dir,'/x_warp.tiff']);
writeftif(y, [dir,'/y_warp.tiff']);


if(1)
    addpath(genpath('..'));
    I1 = imread([dir,'/',I1_name,'.jpg']);
    I2 = imread([dir,'/',I2_name,'.jpg']);
    warp = warpFLColor(I1, I2, vx, vy);
    imwrite(warp,[dir,'/__test_only__',I2_name,'_warped.jpg']);
end