awk '
NR==1 {next}
NR==2 {w=$1; next}
{
  for(i=1;i<=NF;i++){
    printf "%s", ((colset<57 && row <29) || $i=="0")? " " : "1"
    colset++
    if(colset==w){ printf "\n"; colset=0; row++ }
  }
}
' png.pbm > fixed3.pbm
