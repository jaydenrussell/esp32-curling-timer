import sys
sys.path.insert(0,'.')
p='esphome/curling-timer.yaml'
s=open(p).read()
def comment_section(text,key):
    lines=text.splitlines()
    res=[]
    insec=False
    for line in lines:
        if not insec and line.strip()==key+":":
            insec=True
            res.append("# "+line)
            continue
        if insec:
            if line.strip()=='':
                res.append(line); continue
            if line and not line.startswith(' ') and not line.startswith('#'):
                insec=False
                res.append(line); continue
            res.append("# "+line); continue
        res.append(line)
    return "\n".join(res)+"\n"
for key in ["esp_ldo","lt8912b","lvgl"]:
    s=comment_section(s,key)
open(p,'w').write(s)
print("done")
