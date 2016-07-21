Function  CompStr(S,mask:string):boolean;
Var I,J     : Integer;
    Ok      : Boolean;
    St,Msk  : String;
Begin
 Inc(ItsFirst);
 I:=1;
 { ONETIME BLOCK                                                           }
 { Begin optimize mask ('?*','*?','**' sequences must be converted to '*') }
 {                     ('@@' sequences must be converted to '@')           }
 If ItsFirst=1 Then
  Begin
   If OkFileMask Then
    Begin
     Mask:=UpCase(Mask);
     S:=UpCase(S)
    End;
   While True Do
    Begin
     J:=Length(Mask);
     While I<Length(Mask) Do
      Begin
       If (Mask[I]='?') And (Mask[I+1]='*') Then
        Delete(Mask,I,1);
       If (Mask[I]='*') And (Mask[I+1]='?') And (I<Length(Mask)) Then
        Delete(Mask,I+1,1);
       If (Mask[I]='*') And (Mask[I+1]='*') And (I<Length(Mask)) Then
        Delete(Mask,I,1);
       If (Mask[I]='@') And (Mask[I+1]='@') And (I<Length(Mask)) Then
        Delete(Mask,I,1);
       Inc(I);
      End;
     If J=Length(Mask) Then Break;
     I:=1;
    End;
 { End of optimize mask convertion }
  End;
 Ok:=True;
 I:=1;
 J:=1;
 While TRUE Do
  Begin
   Case Mask[I] Of
    { Begin comparing string with current mask sign }
     { Star sign     : any match any simbols }
    '*':
      Begin
       Msk:=Copy(Mask,I+1,Length(Mask)-I+1);
       St:=Copy(S,J,Length(S)-J+1);
       While (St<>'') And (NOT CompStr(St,Msk)) Do Delete(St,1,1);
       If (St='') And (Msk<>'') Then Ok:=False Else J:=Pos(St,S);
      End;
     { Question sign : one match any simbol  }
    '?':
     Begin
      If (I=Length(Mask)) And (J<Length(S)) Then Ok:=False;
      If J>Length(S) Then Ok:=False;
      Inc(J);
     End;
     { Dies sign : one match any digit }
    '#':
     Begin
      If Not OkFileMask Then
       Begin
        If (I=Length(Mask)) And (J<Length(S)) Then Ok:=False;
        If J>Length(S) Then Ok:=False;
        If (S[J]>='0') Or (S[J]<='9') Then Inc(J) Else Ok:=False;
       End;
     End;
     { Up sign : one match upcase simbol }
    '^':
     Begin
      If Not OkFileMask Then
       Begin
        If (I=Length(Mask)) And (J<Length(S)) Then Ok:=False;
        If J>Length(S) Then Ok:=False;
        If ((S[J]>='A') Or (S[J]<='Z')) Or ((S[J]>='€') Or (S[J]<='Ÿ')) //#DLS
         Then Inc(J) Else Ok:=False;
       End;
     End;
    '@':
     Begin
      If Not OkFileMask Then
       Begin
        If (I=Length(Mask)) And (J<Length(S)) Then Ok:=False;
        If J>Length(S) Then Ok:=False;
        If S[J]<>' ' Then Ok:=False;
        While S[J]=' ' Do Inc(J);
       End;
     End;
     { Compare simbols }
    Else If Mask[I]<>S[J] Then Ok:=False Else Inc(J);
    { End compareing string with current mask sign }
    End;
   { Result of compareing }
   If J-1>Length(S) Then Ok:=False;
   If Not Ok Then Break;
   Inc(I);
   If I>Length(Mask) Then Break;
  End;
 CompStr:=Ok;
 Dec(ItsFirst);
End;

