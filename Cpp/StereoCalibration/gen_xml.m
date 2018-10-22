clc;
clear;

out = fopen('images.xml','w');
fprintf(out,'<?xml version="1.0"?>\n<opencv_storage>\n<imagelist>\n');

for i = 15:93
    fprintf(out,'"E:/data/StereoPortable/images/0/%05d.png"\n',i);
    fprintf(out,'"E:/data/StereoPortable/images/1/%05d.png"\n',i);
end

fprintf(out,'</imagelist>\n</opencv_storage>\n');