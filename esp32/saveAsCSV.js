function saveAsCSV() {
  var sheet = SpreadsheetApp.openById('1ELHCl-1dL5LFJFQ-sP02MA2dvbyNQNiFwArYep-9QsU').getSheetByName("Sheet1");
  var lastRow = sheet.getLastRow();
  var range = sheet.getRange("C2:D"+lastRow);
  var csv = "";
  var rows = range.getValues();
  for (var i = 0; i < rows.length; i++) {
    csv += rows[i].join(",") + "\n";
  }
  var folder = DriveApp.getFolderById("1nt-Px6BmoRV0BHB6BhUHxzda2JH02xVT");
  var fileName = sheet.getRange("A2").getValue().toString().split(" ").slice(0,4).join("_");
  var file = DriveApp.createFile(fileName + ".csv", csv, MimeType.CSV);
  folder.addFile(file);
}
