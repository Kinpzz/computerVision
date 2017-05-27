function [ output, scaledLaplacian ] = laplacianFiltering( input, w, scale )
% laplacian fitering
%   output 2d grayscale single 0~1
%   input 2d grayscale single 0~1
%   w laplacian mask 2d
    scaledLaplacian = scale .* spatialFiltering(input,w);
    output = input + scaledLaplacian;
    %output = (output - ones(size(output))*min(min(output)))/max(max(output));
end
