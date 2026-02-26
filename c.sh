awk '
NR==1 {next}
NR==2 {w=$1; next}
{
  for(i=1;i<=NF;i++){
    if(colset<57 && row <29) {
        printf "%s", " " 
    } else {
        printf "%s", ($i=="0")? " " : "1"
    }
    colset++
    if(colset==w){ printf "\n"; colset=0; row++ }
  }
}
' c2.pbm > fixed2.pbm
