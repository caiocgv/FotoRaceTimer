Attribute VB_Name = "Módulo1"
Dim tempo As Variant
Dim R As Integer
Sub IMPORT_RESULTS() ' import txt file into excel sheet "Largada"

    Dim wbO As Workbook
    Dim wsI As Worksheet

    
    Set wsI = Sheets("LARGADA") '<~~ Sheet where you want to import
    With Application.FileDialog(msoFileDialogFilePicker)
        .Title = "Selecione o relatório de largada a ser importado."
        .Show
        filepath = .SelectedItems(1)
    End With
    Set wbO = Workbooks.Open(filepath)

    wbO.Sheets(1).Cells.Copy wsI.Cells

    wbO.Close SaveChanges:=False
    
    Set wsI = Sheets("CHEGADA") '<~~ Sheet where you want to import
    With Application.FileDialog(msoFileDialogFilePicker)
        .Title = "Selecione o relatório de chegada a ser importado."
        .Show
        filepath = .SelectedItems(1)
    End With
    Set wbO = Workbooks.Open(filepath)

    wbO.Sheets(1).Cells.Copy wsI.Cells

    wbO.Close SaveChanges:=False

End Sub
Sub Organizar_dados()
Attribute Organizar_dados.VB_ProcData.VB_Invoke_Func = "a\n14"
'Call IMPORT_RESULTS

Sheets("LARGADA").Activate
i = 0
Do
i = i + 1
Cells(1, 1).Activate

Do While ActiveCell.Value <> "" ' loop through all the cells for filtering data
    R = 1
    If IsNumeric(Left(ActiveCell.Value, 1)) And InStr(ActiveCell.Value, ":") Then
        ActiveCell.Offset(0, 1).Value = ActiveCell.Value
        Do Until InStr(ActiveCell.Offset(R, 0).Value, ":") = False
            R = R + 1 'pula as proximas celulas caso tempos diferentes tenham sido registrados sem nome
        Loop
            ActiveCell.Value = UCase(ActiveCell.Offset(R, 0).Value)
            ActiveCell.Offset(R, 0).EntireRow.Delete
            ActiveCell.Offset(1, 0).Activate
    Else
        ActiveCell.EntireRow.Delete
    End If
Loop

Sheets("CHEGADA").Activate
i = i + 1

Loop While i < 3

Sheets("RESULTADOS").Activate
Call classificar
Call Calcular_resultados
'Call PRINTOUT
10:
End Sub
Sub Calcular_resultados()

'recalcula as formulas da ultima planilha
Sheets("RESULTADOS").Activate
Calculate
    
R = 2 'começa na linha 2

Do

LARGADA = Cells(R, 6).Value
CHEGADA = Cells(R, 7).Value
Cells(R, 4).Value = time_diff_cor(LARGADA, CHEGADA)
R = R + 1

Loop While Cells(R, 3).Value <> ""
'10:


Call classificar_tempos
Call Calcular_dif

End Sub
Sub Calcular_dif()
R = 3
Cells(2, 5) = ""
Do

LARGADA = Cells(2, 4).Value
CHEGADA = Cells(R, 4).Value
Cells(R, 5).Value = time_diff(LARGADA, CHEGADA)
R = R + 1

Loop While IsNumeric(Left(WorksheetFunction.Trim(Cells(R, 4).Value), 1))
End Sub
Sub classificar()
Call classificar_largada
Call classificar_chegada
'Call copy_names
End Sub
Sub PRINTOUT()
Sheets("RESULTADOS").Activate
Cells.EntireColumn.AutoFit
Columns("B:C").Select
Selection.EntireColumn.Hidden = True
R = 2
Do Until Cells(R, 3).Value = ""
    R = R + 1
Loop
Range("A1:D" & R - 1).Select
Selection.PRINTOUT Preview:=True, Copies:=1, Collate:=True

End Sub
