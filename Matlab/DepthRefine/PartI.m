clc;
clear;

dir = 'C:\Users\SZ\Desktop\t201900402_patr2\rectify\N1';
disparity_name = 'disparity_vy.tiff';
% pt = [500;600];%x,y
% d = 2000;%cm
E = 27766.47;
K = [ 1.7874415210087313e+03, 0., 1936, 0., 1.7874415210087313e+03, 1.3845e+03, 0., 0., 1.];

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

pt = zeros(height * width, 3);

for i = 1:height
    for j = 1:width
        depthVal = I_depth(i,j);
        vec = Ki * [depthVal * j; depthVal *i; depthVal;1.0];
        pt(i * height + j, :) = vec(1:3);
        I_depth_center(i,j) = norm(vec (1:3),2);
    end
end

% Use real world to modify the depth
% Depth unit: cm

I_depth_center_fixed = I_depth_center * E;
I_depth_center_fixed(I_depth_center_fixed > 50000) = 50000;
imshow(I_depth_center_fixed,[]);

%writeftif(I_depth_center_fixed, [dir,'/','depth_centered_median.tiff']);
%imwrite(uint16(I_depth_center_fixed), [dir,'/','depth_centered_median.png']);


pt = pt .* E;


fileID = fopen([dir,'/','test.off'],'w');
fprintf(fileID,'OFF\n%d 0 0\n', height * width / 16 );
for i = 1:16:height * width 
    fprintf(fileID,'%f %f %f\n', pt(i,1),pt(i,2),pt(i,3));
end
fclose(fileID);