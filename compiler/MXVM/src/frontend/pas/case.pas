program CaseExample;
var
    grade: char;
    score: integer;
begin
    grade := 'B';
    
    case grade of
        'A': writeln('Excellent!');
        'B': writeln('Good job!');
        'C': writeln('Average');
        'D', 'F': writeln('Needs improvement');
    else
        writeln('Invalid grade');
    end;
    
    score := 85;
    case score of
        90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100: 
            writeln('Grade A');
        80, 81, 82, 83, 84, 85, 86, 87, 88, 89: 
            writeln('Grade B');
        70, 71, 72, 73, 74, 75, 76, 77, 78, 79: 
            writeln('Grade C');
    else
        writeln('Grade F');
    end;
end.
