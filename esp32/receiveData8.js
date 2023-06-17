var SS = SpreadsheetApp.openById('1ELHCl-1dL5LFJFQ-sP02MA2dvbyNQNiFwArYep-9QsU');
var str = "";

function doPost(e) {
  var parsedData;

  try {
    parsedData = JSON.parse(e.postData.contents);
  }
  catch (f) {
    return ContentService.createTextOutput("Error in parsing request body: " + f.message);
  }

  if (parsedData !== undefined) {
    var flag = parsedData.format;
    if (flag === undefined) {
      flag = 0;
    }

    var sheet = SS.getSheetByName(parsedData.sheet_name);
    var dataArr = parsedData.values.split(",");
    //var pointsInterval = parsedData.total_points;
    var pointsInterval = 40;
    var i = 0;
    var rowCounter = 0;
    var date_now = Utilities.formatDate(new Date(), 'America/Sao_Paulo', "dd/MM/yyyy");
    var time_now = Utilities.formatDate(new Date(), 'America/Sao_Paulo', "hh:mm:ss a");

    switch (parsedData.command) {

      case "insert_row":

        sheet.insertRows(2);

        sheet.getRange('A2').setValue(date_now);
        sheet.getRange('B2').setValue(time_now);
        sheet.getRange('C2').setValue(voltage);
        sheet.getRange('D2').setValue(current);

        str = "Success";
        SpreadsheetApp.flush();
        break;

      case "append_row":

        for (let i = 0; i < dataArr.length; i++) {

          while (i < pointsInterval / 4) {
            //var value1 = dataArr [1];
            var publish_array = new Array();
            publish_array[0] = date_now;
            publish_array[1] = time_now;
            publish_array[2] = dataArr[i];
            publish_array[3] = dataArr[i + 1];
            sheet.appendRow(publish_array);
            i = i + 2;
          }
          rowCounter = 2;
          while (i >= pointsInterval / 4 && i < 2 * pointsInterval / 4) {
            var publish_array = new Array();
            publish_array[0] = dataArr[i];
            publish_array[1] = dataArr[i + 1];
            sheet.getRange(rowCounter, 5, 1, publish_array.length).setValues([publish_array]); //column E
            //sheet.getRange(sheet.getLastRow() + 1, columnToAppend, 1, rowToAppend.length).setValues([rowToAppend]);
            rowCounter = rowCounter + 1;
            i = i + 2;
          }
          rowCounter = 2;
          while (i >= 2 * pointsInterval / 4 && i < 3 * pointsInterval / 4) {
            var publish_array = new Array();
            publish_array[0] = dataArr[i];
            publish_array[1] = dataArr[i + 1];
            sheet.getRange(rowCounter, 7, 1, publish_array.length).setValues([publish_array]); //column G
            //sheet.getRange(sheet.getLastRow() + 1, columnToAppend, 1, rowToAppend.length).setValues([rowToAppend]);
            rowCounter = rowCounter + 1;
            i = i + 2;
          }
          rowCounter = 2;
          while (i >= 3 * pointsInterval / 4 && i < 4 * pointsInterval / 4) {
            var publish_array = new Array();
            publish_array[0] = dataArr[i];
            publish_array[1] = dataArr[i + 1];
            sheet.getRange(rowCounter, 9, 1, publish_array.length).setValues([publish_array]); //column I
            //sheet.getRange(sheet.getLastRow() + 1, columnToAppend, 1, rowToAppend.length).setValues([rowToAppend]);
            rowCounter = rowCounter + 1;
            i = i + 2;

          }
          rowCounter = 2;
          while (i >= 4 * pointsInterval / 4 && i < 5 * pointsInterval / 4) {
            var publish_array = new Array();
            publish_array[0] = dataArr[i];
            publish_array[1] = dataArr[i + 1];
            sheet.getRange(rowCounter, 11, 1, publish_array.length).setValues([publish_array]); //column K
            //sheet.getRange(sheet.getLastRow() + 1, columnToAppend, 1, rowToAppend.length).setValues([rowToAppend]);
            rowCounter = rowCounter + 1;
            i = i + 2;
          }
          rowCounter = 2;
          while (i >= 5 * pointsInterval / 4 && i < 6 * pointsInterval / 4) {
            var publish_array = new Array();
            publish_array[0] = dataArr[i];
            publish_array[1] = dataArr[i + 1];
            sheet.getRange(rowCounter, 13, 1, publish_array.length).setValues([publish_array]); //column M
            //sheet.getRange(sheet.getLastRow() + 1, columnToAppend, 1, rowToAppend.length).setValues([rowToAppend]);
            rowCounter = rowCounter + 1;
            i = i + 2;

          }
          rowCounter = 2;
          while (i >= 6 * pointsInterval / 4 && i < 7 * pointsInterval / 4) {
            var publish_array = new Array();
            publish_array[0] = dataArr[i];
            publish_array[1] = dataArr[i + 1];
            sheet.getRange(rowCounter, 15, 1, publish_array.length).setValues([publish_array]); //column O
            //sheet.getRange(sheet.getLastRow() + 1, columnToAppend, 1, rowToAppend.length).setValues([rowToAppend]);
            rowCounter = rowCounter + 1;
            i = i + 2;

          }
          rowCounter = 2;
          while (i >= 7 * pointsInterval / 4) {
            var publish_array = new Array();
            publish_array[0] = dataArr[i];
            publish_array[1] = dataArr[i + 1];
            sheet.getRange(rowCounter, 17, 1, publish_array.length).setValues([publish_array]); //column Q
            //sheet.getRange(sheet.getLastRow() + 1, columnToAppend, 1, rowToAppend.length).setValues([rowToAppend]);
            rowCounter = rowCounter + 1;
            i = i + 2;

          }
        }

        str = "Success";
        SpreadsheetApp.flush();
        break;

      case "sensor":
        var publish_array = new Array();
        publish_array[0] = date_now;
        publish_array[1] = time_now;
        publish_array[2] = dataArr[0];
        publish_array[3] = dataArr[1];
        publish_array[4] = dataArr[2];
        sheet.appendRow(publish_array);

        str = "Success";
        SpreadsheetApp.flush();
        break;

    }

    return ContentService.createTextOutput(str) + i;
  }

  else {
    return ContentService.createTextOutput("Error! Request body empty or in incorrect format.");
  }
}