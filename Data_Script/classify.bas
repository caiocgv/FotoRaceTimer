Attribute VB_Name = "Módulo3"
Sub classificar_largada()
Attribute classificar_largada.VB_ProcData.VB_Invoke_Func = " \n14"

    ActiveWorkbook.Worksheets("LARGADA").Sort.SortFields.Clear
    ActiveWorkbook.Worksheets("LARGADA").Sort.SortFields.Add2 Key:=Range("A1:A500") _
        , SortOn:=xlSortOnValues, Order:=xlAscending, DataOption:=xlSortNormal
    With ActiveWorkbook.Worksheets("LARGADA").Sort
        .SetRange Range("A1:B500")
        .Header = xlNo
        .MatchCase = False
        .Orientation = xlTopToBottom
        .SortMethod = xlPinYin
        .Apply
    End With
End Sub
Sub classificar_chegada()
Attribute classificar_chegada.VB_ProcData.VB_Invoke_Func = " \n14"

    ActiveWorkbook.Worksheets("CHEGADA").Sort.SortFields.Clear
    ActiveWorkbook.Worksheets("CHEGADA").Sort.SortFields.Add2 Key:=Range("A1:A500") _
        , SortOn:=xlSortOnValues, Order:=xlAscending, DataOption:=xlSortNormal
    With ActiveWorkbook.Worksheets("CHEGADA").Sort
        .SetRange Range("A1:B500")
        .Header = xlNo
        .MatchCase = False
        .Orientation = xlTopToBottom
        .SortMethod = xlPinYin
        .Apply
    End With
End Sub
Sub copy_names()
Attribute copy_names.VB_ProcData.VB_Invoke_Func = " \n14"

    Worksheets("CHEGADA").Activate
    Range("A1:A499").Select
    Selection.Copy
    ActiveSheet.Previous.Select
    Range("A1:A499").Select
    ActiveSheet.Paste
    Worksheets("RESULTADOS").Activate
    Range("A2:A500").Select
    ActiveSheet.Paste
    
End Sub
Sub classificar_tempos()

    ActiveWorkbook.Worksheets("RESULTADOS").Sort.SortFields.Clear
    ActiveWorkbook.Worksheets("RESULTADOS").Sort.SortFields.Add2 Key:=Range("D2:D500") _
        , SortOn:=xlSortOnValues, Order:=xlAscending, DataOption:=xlSortNormal
    With ActiveWorkbook.Worksheets("RESULTADOS").Sort
        .SetRange Range("A2:G500")
        .Header = xlNo
        .MatchCase = False
        .Orientation = xlTopToBottom
        .SortMethod = xlPinYin
        .Apply
    End With
End Sub
