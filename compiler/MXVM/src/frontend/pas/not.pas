program NotTest;
var
    x, y: boolean;
begin
    x := true;
    y := false;
    
    if not x then
        writeln('x is false')
    else
        writeln('x is true');
        
    if not (x and y) then
        writeln('not (x and y) is true');
        
    if (not x) or y then
        writeln('(not x) or y evaluated');
end.
