function matlab2opencv( variable, fileName, flag)

[rows, cols] = size(variable);

% Beware of Matlab's linear indexing
variable = variable';

% Write mode as default
if ( ~exist('flag','var') )
    flag = 'w'; 
end

if ( ~exist(fileName,'file') || flag == 'w' )
    % New file or write mode specified 
    file = fopen( fileName, 'w');
    fprintf( file, '%%YAML:1.0\n');
else
    % Append mode
    file = fopen( fileName, 'a');
end

% Write variable header
fprintf( file, '    %s: !!opencv-matrix\n', inputname(1));
fprintf( file, '        rows: %d\n', rows);
fprintf( file, '        cols: %d\n', cols);
fprintf( file, '        dt: f\n');
fprintf( file, '        data: [ ');

% Write variable data
for i=1:rows*cols
    fprintf( file, '%.6f', variable(i));
    if (i == rows*cols), break, end
    fprintf( file, ', ');
    if mod(i+1,4) == 0
        fprintf( file, '\n            ');
    end
end

fprintf( file, ']\n');

fclose(file);

% %YAML:1.0
% Variable1: !!opencv-matrix
% rows: 4
% cols: 5
% dt: f
% data: [ -1.60522782e-03, -5.93489595e-03, 2.92204670e-03,
%     1.14785777e-02, -1.57432575e-02, -2.17529312e-02, 4.05947529e-02,
%     6.56594411e-02, 1.24527821e-02, 3.19751091e-02, 5.41692637e-02,
%     4.04683389e-02, 2.59191263e-03, 1.15112308e-03, 1.11024221e-02,
%     4.03668173e-03, -3.19138430e-02, -9.40114353e-03, 4.93452176e-02,
%     5.73473945e-02 ]
% Variable2: !!opencv-matrix
% rows: 7
% cols: 2
% dt: f
% data: [ -2.17529312e-02, 4.05947529e-02, 5.73473945e-02,
%     6.56594411e-02, 1.24527821e-02, 3.19751091e-02, 5.41692637e-02,
%     4.03668173e-03, -3.19138430e-02, -9.40114353e-03, 4.93452176e-02,
%     4.04683389e-02, 2.59191263e-03, 1.15112308e-03 ]