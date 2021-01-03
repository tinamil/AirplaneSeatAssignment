$myLog = $PSScriptRoot + '/sensitivity_logs/' 
$input = $PSScriptRoot + '/sensitivity_input/'

$stdErrLog = $myLog + 'stderr.log'
$stdOutLog = $myLog + 'stdout.log'

$files = ("Boeing 717-200(717).graph")
foreach($model_type in $model_types)
{
    For ($val=1; $val -lt 111; ++$val)
    {
        foreach ($input_file in $files)
        {
            $output_file = $myLog + $val + ".txt"
            $error_file = $myLog + $input_file + ".err"
            $input_file_name = -join ("`"", $input, $input_file, $val, ".txt", "`"")
            #Write-Output $output_file
            Start-Process -File "${PSScriptRoot}/Mismatch_type_coughing.exe" -ArgumentList $input_file_name  -RedirectStandardOutput $output_file -RedirectStandardError $stdErrLog -wait -WindowStyle Hidden
            $risk = gc $output_file
            Write-Output $risk 
        }
    }
}