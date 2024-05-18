# INERT DATA

for((i = 1; i <= 20; i++))
do 
    ./client --add --name "test$i" --number "100$i" --date "2000.05.0$i" --position "software_engineer" --department "fenliu"
done