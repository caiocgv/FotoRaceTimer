Attribute VB_Name = "Módulo2"
Dim LARGADA As Variant
Dim CHEGADA As Variant
Function time_diff_cor(LARGADA, CHEGADA)
    
LARGADA = Split(LARGADA, ":")
CHEGADA = Split(CHEGADA, ":")

'filtra erro de leitura da placa caso millis>1000
LARGADA(3) = LARGADA(3) Mod 1000
CHEGADA(3) = CHEGADA(3) Mod 1000

'compara tempos da casa de milissegundos
LARGADA(3) = CHEGADA(3) + 549 - LARGADA(3)
'alterar valor da constante conforme necessario
'de acordo com a variação medida entre as fotocelulas

If CInt(LARGADA(3)) < -1000 Then
    LARGADA(3) = LARGADA(3) + 2000
    LARGADA(2) = LARGADA(2) + 2
ElseIf CInt(LARGADA(3)) < 0 Then
    LARGADA(3) = LARGADA(3) + 1000
    LARGADA(2) = LARGADA(2) + 1
ElseIf CInt(LARGADA(3)) > 1000 Then
    LARGADA(3) = LARGADA(3) - 1000
    LARGADA(2) = LARGADA(2) - 1
End If
If CInt(LARGADA(3)) < 100 Then LARGADA(3) = "0" & CStr(LARGADA(3))
'If CInt(LARGADA(3)) < 10 Then LARGADA(3) = "00" & CStr(LARGADA(3)) 'inclui zeros a esquerda caso necessário para manter a formatação dos resultados

'compara tempos da casa de segundos e corrige
'possíveis erros de outra forma
LARGADA(2) = CHEGADA(2) - LARGADA(2) + 1 'alterar valor da constante conforme necessário
If CInt(LARGADA(2)) < 0 Then
    LARGADA(2) = LARGADA(2) + 60
    LARGADA(1) = LARGADA(1) + 1
End If
If CInt(LARGADA(2)) < 10 Then LARGADA(2) = "0" & CStr(LARGADA(2))
 
'compara tempos da casa de minutos e corrige
'possíveis erros
LARGADA(1) = CHEGADA(1) - LARGADA(1)
If CInt(LARGADA(1)) < 0 Then
    LARGADA(1) = LARGADA(1) + 60
    LARGADA(0) = LARGADA(0) + 1
End If
If CInt(LARGADA(1)) < 10 Then LARGADA(1) = "0" & CStr(LARGADA(1)) 'inclui zeros a esquerda caso necessário para manter a formatação dos resultados
    
'compara tempos da casa de horas e corrige
'possíveis erros
LARGADA(0) = CHEGADA(0) - LARGADA(0)
If CInt(LARGADA(0)) < 0 Then
    LARGADA(0) = LARGADA(0) + 24
End If
If CInt(LARGADA(0)) < 10 Then LARGADA(0) = "0" & CStr(LARGADA(0)) 'inclui zeros a esquerda caso necessário para manter a formatação dos resultados

time_diff_cor = CStr(LARGADA(0)) & ":" & CStr(LARGADA(1)) & ":" & CStr(LARGADA(2)) & ":" & CStr(LARGADA(3))

End Function
Function time_diff(LARGADA, CHEGADA)
    
LARGADA = Split(LARGADA, ":")
CHEGADA = Split(CHEGADA, ":")

'filtra erro de leitura da placa caso millis>1000
LARGADA(3) = LARGADA(3) Mod 1000
CHEGADA(3) = CHEGADA(3) Mod 1000

'compara tempos da casa de milissegundos
LARGADA(3) = CHEGADA(3) + 0 - LARGADA(3) 'alterar valor da constante conforme necessario
If CInt(LARGADA(3)) < -1000 Then
    LARGADA(3) = LARGADA(3) + 2000
    LARGADA(2) = LARGADA(2) + 2
ElseIf CInt(LARGADA(3)) < 0 Then
    LARGADA(3) = LARGADA(3) + 1000
    LARGADA(2) = LARGADA(2) + 1
ElseIf CInt(LARGADA(3)) > 1000 Then
    LARGADA(3) = LARGADA(3) - 1000
    LARGADA(2) = LARGADA(2) - 1
End If
If CInt(LARGADA(3)) < 100 Then LARGADA(3) = "0" & CStr(LARGADA(3))
'If CInt(LARGADA(3)) < 10 Then LARGADA(3) = "00" & CStr(LARGADA(3)) 'inclui zeros a esquerda caso necessário para manter a formatação dos resultados

'compara tempos da casa de segundos e corrige
'possíveis erros de outra forma
LARGADA(2) = CHEGADA(2) - LARGADA(2) + 0 'alterar valor da constante conforme necessário
If CInt(LARGADA(2)) < 0 Then
    LARGADA(2) = LARGADA(2) + 60
    LARGADA(1) = LARGADA(1) + 1
End If
If CInt(LARGADA(2)) < 10 Then LARGADA(2) = "0" & CStr(LARGADA(2))
 
'compara tempos da casa de minutos e corrige
'possíveis erros
LARGADA(1) = CHEGADA(1) - LARGADA(1)
If CInt(LARGADA(1)) < 0 Then
    LARGADA(1) = LARGADA(1) + 60
    LARGADA(0) = LARGADA(0) + 1
End If
If CInt(LARGADA(1)) < 10 Then LARGADA(1) = "0" & CStr(LARGADA(1)) 'inclui zeros a esquerda caso necessário para manter a formatação dos resultados
    
'compara tempos da casa de horas e corrige
'possíveis erros
LARGADA(0) = CHEGADA(0) - LARGADA(0)
If CInt(LARGADA(0)) < 0 Then
    LARGADA(0) = LARGADA(0) + 24
End If
If CInt(LARGADA(0)) < 10 Then LARGADA(0) = "0" & CStr(LARGADA(0)) 'inclui zeros a esquerda caso necessário para manter a formatação dos resultados

time_diff = CStr(LARGADA(0)) & ":" & CStr(LARGADA(1)) & ":" & CStr(LARGADA(2)) & ":" & CStr(LARGADA(3))

End Function
