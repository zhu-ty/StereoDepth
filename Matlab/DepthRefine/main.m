clc;
clear;
load('matlab.mat');
%resize
vx_r = imresize(vx, [3008,4112]);
vy_r = imresize(vy, [3008,4112]);
%似乎需要绝对值
vx_ra = abs(vx_r);
%换算成深度？
vx_rad = 1 ./ vx_ra;
%处理成实际距离，[抹去天空]（单位：cm）
vx_radn = vx_rad * (4100 / vx_rad(1300,710));
vx_radnn = vx_radn;
vx_radnn(vx_radn > 100000) = 100000;
%vx_radnn(1:1180,1000:4112) = 100000;

%处理成到中心点距离
K = eye(4);
K(1,1) = 972.00 * (norm([3008,4112],2) / norm([2000,1500],2));
K(1,3) = 2056;
K(2,3) = 1504;
K(2,2) = 972.00 * (norm([3008,4112],2) / norm([2000,1500],2));
Ki = inv(K);
I_depth_center = vx_radnn;
for i = 1:3008
    for j = 1:4112
        depthVal = vx_radnn(i,j);
        vec = Ki * [depthVal * j; depthVal *i; depthVal;1.0];
        I_depth_center(i,j) = norm(vec (1:3),2);
    end
end

%输出
imwrite(uint16(I_depth_center),'depth_c.png');

smooth = depth_refine(imread('00.jpg'), I_depth_center);

imwrite(uint16(smooth),'depth_c_smooth.png');
imshow(uint16(smooth),[]);