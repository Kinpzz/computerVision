function [ output, T ] = histEqualization( input )
%perform histogram equalization
%   output enhanced photo uint8 [0,255]
%   input photo uint8 [0,255]
    row = size(input,1);
    col = size(input,2);
    T = uint8(zeros(1,256));
    output = uint8(zeros(row,col));
    
    pr = imageHist(input)./(col*row);
    % 255*cdf(r)
    for i = 1: 256
        T(1,i) = round(255 * sum(pr(1,1:i)));
    end
    % mapping
    for i = 1:row
        for j = 1:col
            output(i,j) = T(1,input(i,j)+1);
        end
    end
end