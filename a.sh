# handling a p1 pbm file
awk '
NR==1 {print; next}
NR==2 {print; w=$1; next}
{
  for(i=1;i<=NF;i++){
    printf "%s", $i
    count++
    if(count==w){ printf "\n"; count=0 }
  }
}
' c2.pbm > fixed.pbm
