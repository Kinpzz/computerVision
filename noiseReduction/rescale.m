function [ output ] = rescale( input )
    mx = max(input(:));
    mn = min(input(:));
    output = (input-mn)/(mx-mn);
end