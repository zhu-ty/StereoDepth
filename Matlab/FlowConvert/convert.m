clc;
clear;
dir = 'E:/data/stereo/test';
load([dir,'/flow.mat']);


%rows = 3008;%2056;%3008
%cols = 4112;%2464;%4112

rows = 2056;%3008
cols = 2464;%4112

vx_r = imresize(vx, [rows,cols]);
vy_r = imresize(vy, [rows,cols]);

[M,N]=size(vx_r);
[x,y]=meshgrid(1:N,1:M);
x = x + vx_r;
y = y + vy_r;

% matlab2opencv( x, 'warp.yml');
% matlab2opencv( y, 'warp.yml', 'a'); % append mode passed by 'a' flag

%cv::Mat t = cv::imread("E:/data/stereo/No1/x.tiff", cv::IMREAD_ANYDEPTH);
writeftif(x, [dir,'/x.tiff']);
writeftif(y, [dir,'/y.tiff']);


if(1)
    addpath('..');
    I1 = imresize(imread([dir,'/rectify0.jpg']), [rows,cols]);
    I2 = imresize(imread([dir,'/rectify1.jpg']), [rows,cols]);
    warp = warpFLColor(I1, I2, vx_r, vy_r);
    imwrite(warp,[dir,'/I2_warped.jpg']);
end