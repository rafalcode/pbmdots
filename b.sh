awk '
NR==1 {next}
NR==2 {w=$1; next}
{
  for(i=1;i<=NF;i++){
    printf "%s", ($i=="0")? " " : "1"
    count++
    if(count==w){ printf "\n"; count=0 }
  }
}
' c2.pbm > fixed.pbm
