//
// Created by Lukas Valine on 3/21/21.
//

#include "ui/zcalculatorviewcontroller.h"
#include "ui/ztextfield.h"
#include "utils/casutil.h"

void ZCalculatorViewController::onCreate() {
    ZViewController::onCreate();

    auto* field = new ZTextField(this);
    auto* output = new ZLabel("Output", this);

    int yOffset = 30;
    int start = 30;
    int buttonWidth = 100;
    int margin = 10;
    auto* evaluate = new ZButton("Evaluate", this);
    evaluate->setXOffset(start);
    evaluate->setBackgroundColor(highlight);
    evaluate->setYOffset(yOffset);
    evaluate->setMaxWidth(buttonWidth);
    evaluate->setGravity(Gravity::bottomRight);
    evaluate->setOnClick([output, field](){
        string result = CasUtil::get().evaluate(field->getText());
        output->setText(CasUtil::get().evaluate(field->getText()));
    });

    field->setOffset(start + buttonWidth + margin,yOffset);
    field->setMaxWidth(start + buttonWidth + margin + 150);
    field->setMaxHeight(evaluate->getMaxHeight());
    field->setElevation(1.0);
    field->setOutlineType(WireType::outline);
    field->setGravity(Gravity::bottomRight);
    field->setMarginLeft(start);
    field->setOnReturn([output, field](string content){
        string result = CasUtil::get().evaluate(field->getText());
        output->setText(CasUtil::get().evaluate(field->getText()));
    });
    setBackgroundColor(bg);

    output->setYOffset(field->getOffsetY() + evaluate->getHeight() + margin);
    output->setXOffset(evaluate->getLocalRight() + margin);
    output->setGravity(Gravity::bottomRight);
    output->setMaxWidth(field->getMaxWidth());
    output->setMarginLeft(start);
}
