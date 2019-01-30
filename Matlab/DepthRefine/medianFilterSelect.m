function [mask,imout] = medianFilterSelect(im,filterSize,thresUP)
%MEDIANFILTERSELECT 此处显示有关此函数的摘要
%   I, 35, 1.2

mask = ones(size(im));

offs = floor(filterSize/2);
imex = wextend('2D','sym',im ,offs);
progress = 1;
for i = 1+offs:size(im,1)+offs
    for j = 1+offs:size(im,2)+offs
        pickM = imex(i-offs:i+offs,j-offs:j+offs);
        pickM = reshape(pickM,1,[]);
        me = median(pickM);
        %av = mean(pickM);
        va = imex(i,j);
        if(va > thresUP * me)
            mask(i-offs,j-offs) = 0;
            %imout(i-offs,j-offs) = av;
        end
    end
    if((i -offs) / size(im,1) * 100 > progress)
        fprintf('%1.1f%%\n',(i -offs) / size(im,1) * 100);
        progress =progress + 1;
    end
end
imout = regionfill(im, ~logical(mask));
end

