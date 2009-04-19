% File: map_0_255.m
% Author: ryanlei
% Creation: 2009/04/19
% Modification: XXXX/XX/XX
% Description: Map the image to 0~255 index range.

function [ result ] = map_0_255 ( img )

result = double( img );
MAX = max( result(:) );
MIN = min( result(:) );

result = (img - MIN) / (MAX - MIN) * 255; 

result = uint8( result );

end