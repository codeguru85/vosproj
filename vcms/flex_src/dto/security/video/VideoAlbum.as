package dto.security.video
{
	import mx.collections.ArrayCollection;
	[Bindable]
	[RemoteClass(alias="com.broadengate.vcms.entity.VideoAlbum")]
	public class VideoAlbum
	{
		public var albumId:int;
		public var albumName:String;
		public var videoNum:int;
		public var createTime:Date;
		public var descn:String;
		public var imagePath:String;
		
		public function VideoAlbum()
		{
			//TODO: implement function
		}
	}
}