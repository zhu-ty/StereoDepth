k = 1.4474103e+05;


for i = 1:10
    
    fprintf('working on %d ...\n', i);
    
    dir = ['../',num2str(i)];
    
    I_dis = imread([dir, '/disparity.tiff']);
    I_d = 1 ./ I_dis;
    I_d2 = I_d .* k;
    I_d2(I_d2 > 50000) = 50000;
    writeftif(I_d2, [dir, '/depth.tiff']);
end