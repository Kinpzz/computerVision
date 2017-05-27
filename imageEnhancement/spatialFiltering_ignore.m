function [ output ] = spatialFiltering_zeros( input, w )
%sptial filtering using ignore edges methods
%   input grayscale image single 0~1
%   w:mask 2d mask single
%   output grayscale image single 0~1
    extra_row = (size(w,1)-1)/2;
    extra_col = (size(w,2)-1)/2;
    output = input;
    
    for i = extra_row+1:size(input, 1)-extra_row
        for j = extra_col+1:size(input,2)-extra_col
            output(i,j) = 0;
            for m = 1 : size(w,1)
                for n = 1 : size(w,2)
                    output(i,j) = output(i,j) + w(m,n)*input(...
                        (size(w,1)+1)/2-m+i, (size(w,2)+1)/2-n+j);
                end
            end
        end
    end
end

