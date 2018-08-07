function writeftif(outim,outfilename)
t = Tiff(outfilename,'w');
if size(outim,3)==3
    t.setTag('Photometric',Tiff.Photometric.RGB);
elseif size(outim,3)==1
    t.setTag('Photometric',Tiff.Photometric.MinIsBlack);
end
t.setTag('Compression',Tiff.Compression.None);
t.setTag('BitsPerSample',32);
t.setTag('SamplesPerPixel',size(outim,3));
t.setTag('SampleFormat',Tiff.SampleFormat.IEEEFP);
t.setTag('ImageLength',size(outim,1));
t.setTag('ImageWidth',size(outim,2));
t.setTag('PlanarConfiguration',Tiff.PlanarConfiguration.Chunky);
% Write the data to the Tiff object.
t.write(single(outim));
t.close();