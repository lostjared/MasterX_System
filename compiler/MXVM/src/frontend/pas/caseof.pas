program CaseEx;
var
x: integer;
begin
    x := 0;
    writeln('enter number: ');
    readln(x);
    case x of
        0: writeln('value');
        50: writeln('its 50');
    else
        writeln('invalid number');
    end;
end.
