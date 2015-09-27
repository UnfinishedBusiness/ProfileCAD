#include <UI.h>

using namespace std;

vector<uiEntity> uiEntityArray;
int uiEntityArrayIndex;

void uiInit()
{
  uiEntityArrayIndex = 0;
  uiAppend(uiEntity{UI_TEXT, WHITE, "", UI_MENU_POSITION}); //Msg text - index 0
  uiAppend(uiEntity{UI_TEXT, WHITE, "", UI_INPUT_POSITION}); //Msg text - index 1
  uiAppend(uiEntity{UI_TEXT, WHITE, "", UI_HINT_POSITION}); //Mouse Position - index 2
  uiAppend(uiEntity{UI_TEXT, WHITE, "", UI_MOUSE_POSITION}); //Mouse Position - index 3
  uiAppend(uiEntity{UI_TEXT, WHITE, "", UI_SNAP_POSITION}); //Snap Position - index 4
  uiSnapIndicator();
  cliMenu();
}
void uiAppend(uiEntity e)
{
  uiEntityArray.push_back(uiEntity());
  uiEntityArray[uiEntityArrayIndex] = e;
  uiEntityArrayIndex++;
  glutPostRedisplay();
}
void uiEdit(int x, uiEntity e)
{
  if (uiEntityArrayIndex > x) //Make sure entity exists
  {
    uiEntityArray[x] = e;
  }
  glutPostRedisplay();
}
void uiDrawPNG(string file, int w, int h)
{
  GLuint theTexture = uiLoadPNG(file.c_str(), w, h);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the back buffer
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, theTexture);
  glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 1.0f);
      glVertex3f(-2, 2, -3);
      glTexCoord2f(1.0f, 1.0f);
      glVertex3f(2, 2, -3);
      glTexCoord2f(1.0f, 0.0f);
      glVertex3f(2, -2, -3);
      glTexCoord2f(0.0f, 0.0f);
      glVertex3f(-2, -2, -3);
  glEnd();
}
void uiDrawText(void * font, char *s, float x, float y, float z)
{
  //D printf("(uiDrawText) s = %s\n", s);
  unsigned int i;
  glRasterPos3f(x, y, z);
  for (i = 0; i < strlen (s); i++)
  {
    glutBitmapCharacter (font, s[i]);
  }
}
void uiSnapIndicator()
{
  std::vector<std::string> snap = mouseUISnapIndicator();
  std::string s = "";
  if (snap.size() > 0)
  {
    s = snap[0];
    for (int x = 1; x < snap.size(); x++)
    {
      s.append(" | ");
      s.append(snap[x]);
      //cout << snap[x] << endl;
    }
  }
  V cout << KRED << "(uiSnapIndicator) => " << KGREEN << s << KNORMAL << endl;
  uiEdit(4, uiEntity{UI_TEXT, RED, s , UI_SNAP_POSITION});
}
void uiRender()
{
  //D printf("(uiRender)\n");
  for (int i = 0; i < uiEntityArrayIndex; i++)
  {
      if (uiEntityArray[i].Type == UI_TEXT)
      {
        //glScalef(5, 5, 0);
        sceneColor(uiEntityArray[i].Color);
        uiDrawText(GLUT_BITMAP_HELVETICA_12, (char *)uiEntityArray[i].Text.c_str(), uiEntityArray[i].Position.x * .001, uiEntityArray[i].Position.y * .001, uiEntityArray[i].Position.z * .001);
      }

  }
}
GLuint uiLoadPNG(const char * file_name, int w, int h)
{
    int *width = &w;
    int *height = &h;
    png_byte header[8];

    FILE *fp = fopen(file_name, "rb");
    if (fp == 0)
    {
        perror(file_name);
        return 0;
    }

    // read the header
    fread(header, 1, 8, fp);

    if (png_sig_cmp(header, 0, 8))
    {
        fprintf(stderr, "error: %s is not a PNG.\n", file_name);
        fclose(fp);
        return 0;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        fprintf(stderr, "error: png_create_read_struct returned 0.\n");
        fclose(fp);
        return 0;
    }

    // create png info struct
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        fprintf(stderr, "error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        fclose(fp);
        return 0;
    }

    // create png info struct
    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info)
    {
        fprintf(stderr, "error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        fclose(fp);
        return 0;
    }

    // the code in this if statement gets called if libpng encounters an error
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "error from libpng\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return 0;
    }

    // init png reading
    png_init_io(png_ptr, fp);

    // let libpng know you already read the first 8 bytes
    png_set_sig_bytes(png_ptr, 8);

    // read all the info up to the image data
    png_read_info(png_ptr, info_ptr);

    // variables to pass to get info
    int bit_depth, color_type;
    png_uint_32 temp_width, temp_height;

    // get info about png
    png_get_IHDR(png_ptr, info_ptr, &temp_width, &temp_height, &bit_depth, &color_type,
        NULL, NULL, NULL);

    if (width){ *width = temp_width; }
    if (height){ *height = temp_height; }

    // Update the png info struct.
    png_read_update_info(png_ptr, info_ptr);

    // Row size in bytes.
    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    // glTexImage2d requires rows to be 4-byte aligned
    rowbytes += 3 - ((rowbytes-1) % 4);

    // Allocate the image_data as a big block, to be given to opengl
    png_byte * image_data;
    image_data = (png_bytep)malloc(rowbytes * temp_height * sizeof(png_byte)+15);
    if (image_data == NULL)
    {
        fprintf(stderr, "error: could not allocate memory for PNG image data\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return 0;
    }

    // row_pointers is for pointing to image_data for reading the png with libpng
    png_bytep * row_pointers = (png_bytep*)malloc(temp_height * sizeof(png_bytep));
    if (row_pointers == NULL)
    {
        fprintf(stderr, "error: could not allocate memory for PNG row pointers\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        free(image_data);
        fclose(fp);
        return 0;
    }

    // set the individual row_pointers to point at the correct offsets of image_data
    int i;
    for (i = 0; i < temp_height; i++)
    {
        row_pointers[temp_height - 1 - i] = image_data + i * rowbytes;
    }

    // read the png into image_data through row_pointers
    png_read_image(png_ptr, row_pointers);

    // Generate the OpenGL texture object
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, temp_width, temp_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // clean up
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    free(image_data);
    free(row_pointers);
    fclose(fp);
    return texture;
}
