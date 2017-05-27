function [ output ] = spatialFiltering_zeros( input, w )
%sptial filtering using add zeors methods
%   input grayscale image single 0~1
%   w:mask 2d mask single
%   output grayscale image single 0~1
    extra_row = (size(w,1)-1)/2;
    extra_col = (size(w,2)-1)/2;
    % zeros
    input = [zeros(extra_row,size(input,2));input];
    input = [input;zeros(extra_row,size(input,2))];
    input = [zeros(size(input,1),extra_col),input];
    input = [input,zeros(size(input,1),extra_col)];
    
    output = single(zeros(size(input)));
    
    for i = extra_row+1:size(input, 1)-extra_row
        for j = extra_col+1:size(input,2)-extra_col
            for m = 1 : size(w,1)
                for n = 1 : size(w,2)
                    output(i,j) = output(i,j) + w(m,n)*input(...
                        (size(w,1)+1)/2-m+i, (size(w,2)+1)/2-n+j);
                end
            end
        end
    end
    row = size(output,1);
    col = size(output,2);
    output = output(extra_row+1:row-extra_row, extra_col+1:col-extra_row);
end

