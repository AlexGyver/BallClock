let font_face = '';

async function onfile_h(file) {
    let reader = new FileReader();

    reader.onload = async () => {
        font_face = file.name.split('.')[0];
        const font = new FontFace(font_face, reader.result);
        await font.load();
        document.fonts.add(font);
    };

    reader.onerror = () => console.log(reader.error);
    reader.readAsArrayBuffer(file);
}


function test() {
    let cv = document.getElementById('canvas');
    const context = cv.getContext("2d");
    context.font = "48px " + font_face;
    context.fontWeight = 400;
    context.fontStyle = "normal";
    // context.textAlign = "center";
    // context.textBaseline = "middle";
    context.fillText(font_face, 10, 50);
}